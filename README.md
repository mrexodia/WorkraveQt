# WorkraveQt

Modern reimplementation of [Workrave](https://workrave.org) in Qt. It is designed to help with Repetitive Strain Injury (RSI), but in a much stricter way than the original Workrave implementation.

![break](screenshots/break.png)

![game list](screenshots/gamelist.png) 

![preferences](screenshots/preferences.png) ![timer](screenshots/timer.png)

## Getting started

You can download one of the [releases](https://github.com/mrexodia/WorkraveQt/releases), extract it anywhere and open `WorkraveQt`.

## Building

1. Install a recent version of [Qt](https://www.qt.io/offline-installers) (you can bypass the account requirement by setting `0.0.0.0` as a proxy)
2. (Optional) [Configure Qt Creator](https://doc.qt.io/qtcreator/creator-configuring.html)
3. Open `src/WorkraveQt.pro` in Qt Creator

### Portable Qt installation

To avoid using Qt's official setups that require an account you can use [aqtinstall](https://github.com/miurahr/aqtinstall).

```
pip install aqtinstall
aqt install-qt windows desktop 5.12.12 win64_msvc2017_64
aqt install-tool windows desktop tools_qtcreator_gui
```

You can find more available Qt configurations [here](https://ddalcino.github.io/aqt-list-server/).