# basedfs

## Setup

1. Download [Ubuntu Desktop 14.04 .iso](http://www.ubuntu.com/download/desktop "Ubuntu 14.04")
2. Download and install [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
3. Make a new virtual machine with Virtual Box, loading the .iso downloaded above
4. After the VM has been installed, open the terminal (Ctrl + Alt + t), and enter
 

 ```bash
 sudo apt-get update
 sudo apt-get upgrade
 ```


This might take a few minutes as it updates the machine. While this is running, create a (Github)[https://github.com] account, and follow (this guide) to create ssh keys on your machine. Once you've made an account, (email me)[alexander.koumis@sjsu.edu] your username and I will add you as a collaborator to the repository/project.

Once this is done, install git, which we will use to keep our code in sync:

```bash
sudo apt-get install git
```