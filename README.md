#basedfs

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

# Development flow

The general development process for basedfs will occur as follows:

## Initialize git project

Make a new directory for your git projects and clone into it the master repository: 

```bash
mkdir ~/git
cd ~/git
git clone git@github.com:alexanderkoumis/cmpe142fs.git
```

This will copy the project's contents into ~/git/cmpe142fs.

Each contributor will maintain their own local working branch, which should always be based on the latest code from the master. Enter the following to create your own branch, and make it accessible from the repository's 'branches' button:

```bash
git checkout -b $NAMEOFURBRANCH
git push origin $NAMEOFURBRANCH
```
(The 'branch' button:)

![Branches](http://i.imgur.com/27enzJB.png)

In the terminal, type `git branch` the branches git is currently tracking.

## Making changes to local branch
1. Make necessary edits to code (In personal branch)
2. Once you are satisfied with your changes, tell git to track the file you changed in your next commit (checkpoint). Type `git status` to see a list of files that have changed since your last commit, followed by `git add $NAMEOFFILE` for every file to be tracked. For example, if you changed the file `~/git/cmpe142fs/inode.c`, type `git add ~/git/cmpe142fs/inode.c`
4. Commit the changes to your personal branch using the command `git commit -a -m "Descriptive commit message"`, adding within the parentheses a short description of what is in your most recent commit
5. Check out the master branch and pull the most recent master code, merging it with your working branch, ensuring changes made to the master after you last pulled from the master do not mess up your code:

```bash
git checkout master
git pull
git checkout $NAMEOFURBRANCH
git merge master
```

Notice the absence of the `-b` flag from `git checkout`, which creates a new branch rather than switching to an existing one.

## Merging local branch changes with master

Once we have commits on our local branches that we believe ready to be part of the master, send an email to the group describing your changes, and if it is generally agreed upon, I (Alexander) will merge your code into the master. Others can alter the master as well using a process similar to the one described above, but it is very important to keep a rather sanitized master branch, so designating one individual to handle merges into the master reduces risk for error.

# Building basedfs

A makefile has been setup to compile the project. If your changes have been made entirely within a file already included in the project, simply type the following to build the project's executable:

```make
make clean // This will clean previously compiled object files
make all
```

If you have added a .c file to your branch, open up the `makefile` file in the project's root directory and add the name of the .c file to the existing list in the `all` rule. As all files desired must be tracked by git to make it into the repository, add this .c file using the `git add $FILENAME` command as detailed in the "Making changes to local branch" section. Also keep in mind, to decrease the size of the project, make sure you don't add the build executable files during the `git add` process, as everyone should build these on their own machines.

# Mounting basedfs

First you will need to create an image file and mount point. Go to a directory of your choice:
```bash
dd bs=4096 count=100 if=/dev/zero of=image
mkdir /mount
```
Now to mount we need to insert the kernel module:

```bash
sudo insmod basedfs.ko
sudo mount -o loop -t basedfs image ./mount
```

To check to make sure fs is mounted:
```bash
sudo cat /etc/mtab
```

look at the last line and check for our fs.

To unmount:
```bash
sudo umount ./mount
<<<<<<< HEAD
```
=======
```
>>>>>>> master
