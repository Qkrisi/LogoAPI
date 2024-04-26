from __future__ import annotations
import socket
import select
import threading
import time
import typing
import collections.abc

_START = 0x07
_SEPARATOR = 0x21
_ESCAPE = ['\\', ' ', '[', ']', '(', ')', '"', '+', '-', '/', '*']

def _encode_number(n: int) -> list[int]:
	res = []
	for c in str(n):
		res.append(ord(c))
	return res

def _next_part(message: list[int]) -> None:
	while message.pop(0) != _SEPARATOR:
		pass

def _read_number(message: list[int]) -> int:
	n = ""
	while message[0] != _SEPARATOR:
		n += chr(message.pop(0))
	del message[0]
	return int(n)

def _read_till_end(message: list[int]) -> str:
	msg = ""
	while len(message) > 0 and message[0] != 0x00:
		msg += chr(message.pop(0))
	return msg

def to_string(message: str) -> str:
	for c in _ESCAPE:
		message = message.replace(c, '\\' + c)
	return '"' + message

class MessageTypeSend:
	MESSAGE = 0x57
	COMMAND = 0x52
	RESULT = 0x4C

class Command:
	JOIN = 0x61
	QUERY_CLIENTS = 0x55

class MessageTypeReceive:
	MESSAGE = 0x77
	COMMAND = 0x72
	RESULT = 0x6C

class CommandResponse:
	JOINED = 0x62
	CLIENTS = 0x76

class LogoClient:
	def __init__(self,
				 name: str,
				 host: str,
				 port: int = 51,
				 *,
				 on_message: typing.Callable[[LogoClient, str, int, str], None] = lambda client, sender, message_type, message: None,
				 on_error: typing.Callable[[Exception], None] = lambda error: None,
				 buffer_size: int = 1024,
				 refresh_rate: int = 60
				 ):
		self.__host: str = host
		self.__port: int = port
		self.__origname: str = name
		self.__name: str = None
		self.on_message: typing.Callable[[LogoClient, str, int, str], None] = on_message
		self.on_error: typing.Callable[[Exception], None] = on_error
		self.__buffer_size: int = buffer_size
		self.__refresh_rate: int = refresh_rate
		self.__connected: bool = False
		self.__socket: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.__wait_for_clients: bool = False
		self.__waiting: int = 0
		self.__responses: list[tuple[str, str]] = []
		self.__clients: list[str] = []
		self.__t1: bool = False
		self.__t2: bool = False
		self.__open: bool = True

	def __enter__(self):
		self.connect()
		return self

	def __exit__(self, *args):
		self.close()

	def _raise_if_disconnected(func):
		def raise_if_disconnected_inner(*args, **kwargs):
			if not args[0].__connected:
				raise Exception("Logo client isn't connected")
			return func(*args, **kwargs)
		return raise_if_disconnected_inner

	@property
	@_raise_if_disconnected
	def name(self) -> str:
		return self.__name[:]

	@property
	@_raise_if_disconnected
	def peers(self) -> tuple[str]:
		return tuple(self.__clients)

	@property
	@_raise_if_disconnected
	def clients(self) -> tuple[str]:
		if len(self.__clients) < 2:
			return tuple()
		return tuple(self.__clients[1:])

	@property
	@_raise_if_disconnected
	def server(self) -> typing.Union[str, None]:
		if len(self.clients) == 0:
			return None
		return self.__clients[0]

	@property
	def connected(self) -> bool:
		return self.__connected

	@property
	def closed(self) -> bool:
		return not self.__open

	def _handle_exception(self, e: Exception) -> None:
		if not self.__connected:
			return
		self.on_error(e)
		self.disconnect()

	@_raise_if_disconnected
	def send_raw(self, message_type: int, *parts: str, append: str = "", wait: int = 0) -> typing.Union[tuple[tuple[str, str]], None]:
		if not self.__connected:
			return
		data = [_START]
		parts_data = [_SEPARATOR, message_type] + _encode_number(len(parts)) + [_SEPARATOR]
		for part in parts:
			parts_data += _encode_number(len(part)) + [_SEPARATOR]
			for c in part:
				parts_data.append(ord(c))
		for c in append:
			parts_data.append(ord(c))
		data += _encode_number(len(parts_data) + len(data) - 2) + parts_data
		if wait > 0:
			self.__waiting += wait
		try:
			self.__socket.sendall(bytes(data))
		except Exception as e:
			self._handle_exception(e)
			return None
		if wait > 0:
			while len(self.__responses) < wait:
				pass
			resp = []
			for i in range(wait):
				resp.append(self.__responses.pop(0))
			self.__waiting -= wait
			return tuple(resp)
		return None


	def _receive(self) -> None:
		self.__t1 = True
		while self.__connected:
			try:
				ready, _, _ = select.select([self.__socket], [], [])
				if ready:
					data = self.__socket.recv(self.__buffer_size)
					self._process_message(data)
			except Exception as e:
				self._handle_exception(e)
				break
		self.__t1 = False

	def _process_message(self, message) -> None:
		message = list(message)
		_next_part(message)
		t = message.pop(0)

		if t == CommandResponse.JOINED:
			_next_part(message)
			name = ""
			for c in message:
				name += chr(c)
			self.__name = name
			threading.Thread(target=self._keep_updating_clients).start()
			return

		if t == CommandResponse.CLIENTS:
			clients = []
			for i in range(_read_number(message)):
				name = ""
				for j in range(_read_number(message)):
					name += chr(message.pop(0))
				clients.append(name)
			self.__clients = clients
			self.__wait_for_clients = False
			return

		if t == MessageTypeReceive.MESSAGE or t == MessageTypeReceive.COMMAND or t == MessageTypeReceive.RESULT:
			_next_part(message)
			sender = ""
			for i in range(_read_number(message)):
				sender += chr(message.pop(0))
			msg = _read_till_end(message)
			if t == MessageTypeReceive.RESULT and msg.startswith("OK: "):
				msg = msg[4:]
			if (t == MessageTypeReceive.MESSAGE or t == MessageTypeReceive.RESULT) and self.__waiting > 0:
				self.__responses.append((sender, msg))
			else:
				self.on_message(self, sender, t, msg)
			return

		print(f"Unknown message type: {t}")

	def _keep_updating_clients(self) -> None:
		if self.__refresh_rate < 1 or self.__refresh_rate == None:
			return
		self.__t2 = True
		while self.__connected:
			self.update_clients(True)
			for i in range(self.__refresh_rate):
				time.sleep(1)
				if not self.__connected:
					break
		self.__t2 = False

	def _reset(self) -> None:
		self.__clients = []
		self.__name = None
		self.__waiting = 0
		self.__responses = []
		self.__wait_for_clients = False

	def connect(self) -> None:
		if self.__connected:
			return
		assert self.__open, "Socket has been closed."
		while self.__t1 or self.__t2:
			pass
		self.__socket.connect((self.__host, self.__port))
		self.__connected = True
		threading.Thread(target=self._receive).start()
		self.send_raw(Command.JOIN, append = self.__origname)
		while self.__name == None or len(self.__clients) == 0:
			pass

	def disconnect(self) -> None:
		if not self.__connected:
			return
		self.__connected = False
		self.__socket.shutdown(socket.SHUT_RDWR)
		self._reset()

	def close(self) -> None:
		self.disconnect()
		self.__socket.close()
		self.__open = False

	@_raise_if_disconnected
	def update_clients(self, wait: bool = False) -> None:
		while self.__wait_for_clients:
			pass
		self.__wait_for_clients = wait
		self.send_raw(Command.QUERY_CLIENTS)
		if wait:
			while self.__wait_for_clients:
				pass

	@_raise_if_disconnected
	def send_message(self, message_type: int, message: str, *clients: str, wait: bool = False) -> typing.Union[tuple[tuple[str, str]], None]:
		if message_type == MessageTypeSend.RESULT and not message.startswith("OK: "):
			message = "OK: " + message
		return self.send_raw(message_type, *([self.name] + list(clients)), append = message, wait = len(clients) if wait else 0)