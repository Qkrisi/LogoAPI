# LogoApi

Ez a könyvtár lehetővé teszi a kommunikációt Imagine Logo és külső folyamatok/eszközök között.

Mintaprojektek [itt](https://github.com/qkrisi/LogoApiExamples) találhatóak.

## Előkészületek

A kommunikáció megvalósításához használjuk az Imagine Logo-ba beépített `Háló` osztályt. Ennek az osztálynak a feladata TCP/IP kapcsolat létrehozása két Logo folyamat között. A LogoApi ezt teszi lehetővé más folyamatok számára is.

Az `új "háló []` parancs használatával hozhatunk létre egy új háló objektumot. Ennek hatására az intézőben megjelenik egy `háló1` nevű objektum.

![Háló objektum](https://qkrisi.hu/static/logo/logonetm.png)

Erre duplán kattintva állíthatjuk a kapcsolat beállításait.

Állítsuk a stílust `Szerver` módra, majd a "Csatlakozik" gombra kattintva indíthatjuk el a szervert. Ez a szerver az 51-es porton fog futni.

Ha szeretnénk közvetlenül végrehajtandó parancsokat küldeni a szervernek, jelöljük be a "Végrehajtható" jelölőnégyzetet!

![Háló objektum változtatása](https://qkrisi.hu/static/logo/logonet.png)

## Használat

A LogoApi használatról szoló dokumentáció a repository [wiki](https://github.com/Qkrisi/LogoAPI/wiki) oldalán található.
