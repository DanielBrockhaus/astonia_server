To run under Ubuntu Ubuntu 22.04.4 LTS (tested and working as of 2024.06.27)

```
sudo apt-get update
sudo apt install git
git clone https://github.com/DanielBrockhaus/astonia_server.git
sudo apt-get install gcc-multilib
sudo dpkg --add-architecture i386
sudo apt install make
sudo apt install lib32z-dev
sudo apt install libmysqlclient-dev:i386
sudo apt install mariadb-server
cat MYSQLPASSWD
sudo mysql_secure_installation
# old root password is blank. new root password from 'cat MYSQLPASSWD'.
# do not switch to unix sockets
./my <create_tables.sql 
./my merc <merc.sql 
./my merc <storage.sql 
echo "Welcome to Astonia" >motd.txt
rm MYSQLPASSWD
# the "my" script is very handy, but also a security risk!
```

