# LogoApi

A magyar leírás [itt](https://github.com/Qkrisi/LogoAPI/blob/master/README.md) található.

The LogoApi library enables communication between an instance of Imagine Logo and an external process.

Example projects can be found [here](https://github.com/qkrisi/LogoApiExamples).

## Preparations

To establish the connection, create an instance of the `Net` class built into Imagine. The purpose of this class is to let multiple instances of Imagine communicate a TCP/IP connection. LogoApi lets any other process join this connection.

The Net instance can be created using the `new "net []` command. Upon running it a net object called `net1` will appear in the explorer under MainWindow.

![Net object](https://qkrisi.hu/static/logo/logonetmeng.png)

Double clicking on it lets us change the settings of the object.

Let's set the style to `Server` mode, and start the server using the "Connect" button. The used port will be port 51.

If you wish to send direct commands to the server to execute, check the "RunEnabled" checkbox.

![Net object change window](https://qkrisi.hu/static/logo/logoneteng.png)

## Usage

The documentation of the LogoApi can be found on the [wiki page](https://github.com/Qkrisi/LogoAPI/wiki) of the repository.
