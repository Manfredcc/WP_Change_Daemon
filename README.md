# WP Change Daemon

**Brief description**

This is a wallpaper-changing daemon that starts at the beginning of the system, based on feh. You can replace it with your command on your PC.

> For ubuntu
>
> ```shell
> sudo apt-get install feh
> ```

| Date       | Author  | Description   |
| ---------- | ------- | ------------- |
| 2023/12/04 | Manfred | First release |



## Rough Design

**Features**

- Automatically change the wallpaper at predetermined intervals
- There are 3 methods to switch wallpapers: sequential, random, and by name



**Require**

- Offer a user-friendly interface interface
- Runs well with scripts



## Input/Output

| Input       | Output                                 |
| ----------- | -------------------------------------- |
| 000         | Disable change-wallpaper automatically |
| 001         | Enable change-wallpaper automatically  |
| 010         | Switch **default method** to sequence  |
| 011         | Switch **default method** to random    |
| 100         | Set wallpaper sequentially             |
| 101         | Set wallpaper randomly                 |
| 110         | Restart program                        |
| 111         | Exit program                           |
| 1000 - 1110 | Reserved                               |



## Core structure

























