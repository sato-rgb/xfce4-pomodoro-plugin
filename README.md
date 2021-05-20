[![License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://gitlab.xfce.org/panel-plugins/xfce4-sample-plugin/-/blob/master/COPYING)

# xfce4-pomodoro-plugin

Xfce4-pomodoro-plugin is a pomodoro timer for the Xfce panel.

----

### dependencies

- libnotify
- libxfce4panel-2.0
- libxfce4ui-2

    sudo apt install libnotify-dev libxfce4panel-2.0-dev libxfce4ui-2-dev

### Installation

From source code repository: 

    cd xfce4-pomodoro-plugin
    ./autogen.sh
    make
    sudo make install

if the plugin does not appear in plugin menu, try prefix=/usr:

    cd xfce4-pomodoro-plugin
    ./autogen.sh prefix=/usr
    make
    sudo make install

### Reporting Bugs

use GitHub's feature (I do not know) or mail to my mail address.

