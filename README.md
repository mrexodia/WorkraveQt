# WorkraveQt

Modern reimplementation of Workrave in Qt.

## Building

1. Install a recent version of [Qt](https://www.qt.io/offline-installers) (you can bypass the account requirement by setting `0.0.0.0` as a proxy)
2. (Optional) [Configure Qt Creator](https://doc.qt.io/qtcreator/creator-configuring.html)
3. Open `src/WorkraveQt.pro` in Qt Creator

### Portable Qt (Windows, Visual Studio 2019)

From Git Bash:

```
curl -O -L https://code.qt.io/cgit/qbs/qbs.git/plain/scripts/install-qt.sh 1>nul
sh install-qt.sh --directory /d/Qt --host windows_x86 --target desktop --toolchain win64_msvc2017_64 --version 5.12.9 qt 3d qtactiveqt qtbase qtcanvas3d qtconnectivity qtdeclarative qtgamepad qtgraphicaleffects qtimageformats qtlocation qtm ultimedia qtquickcontrols qtquickcontrols2 qtremoteobjects qtscxml qtsensors qtserialbus qtserialport qtspeech qtsvg qt tools qttranslations qtwebchannel qtwebsockets qtwebview qtwinextras qtxmlpatterns d3dcompiler_47 opengl32sw
```

Make sure to adjust `/d/Qt` to point to a drive of your liking (Qt in the root of a drive is preferred because of potential long paths). Also make sure to install Qt Creator (or use [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2019))