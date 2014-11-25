# basedfs

## Setup VM

1. Download [Ubuntu Desktop 14.04 .iso](http://www.ubuntu.com/download/desktop "Ubuntu 14.04")
2. Download and install [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
3. Make a new virtual machine with Virtual Box, loading the .iso downloaded above

## Update Ubuntu/Install git

After the VM has been installed, open the terminal (Ctrl + Alt + t), and enter
 
 ```bash
 sudo apt-get update
 sudo apt-get upgrade
 ```

This might take a few minutes as it updates the machine. While this is running, create a [Github](https://github.com GitHub) account, and follow [this guide](https://help.github.com/articles/generating-ssh-keys/ generate ssh keys) to create ssh keys on your machine. Once you've made an account, [email me](alexander.koumis@sjsu.edu) your username and I will add you as a collaborator to the repository/project.

Once this is done, install git, which we will use to keep our code in sync:

```bash
sudo apt-get install git
```

## Create local copy of project

Make a new directory on your Ubuntu machine to store your project:

```bash
mkdir ~/git
cd ~/git
git clone git@github.com:alexanderkoumis/cmpe142fs.git
```

This will copy the project's contents into ~/git/cmpe142fs. Each contributor will maintain their own local working branch, which should always be based on the latest code from the master. Enter the following to create your own branch, and push it to the GitHub repository, making it accessible from the 'branches' button:

![Branches](http://i.imgur.com/27enzJB.png)