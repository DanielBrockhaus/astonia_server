To run under Ubuntu (work in progress)

sudo apt install git
git clone https://github.com/DanielBrockhaus/astonia_server.git

sudo apt-get install gcc-multilib
sudo dpkg --add-architecture i386
sudo apt install lib32z-dev
sudo apt install libmysqlclient-dev:i386
sudo apt install mariadb-server
cat MYSQLPASSWD
sudo mysql_secure_installation
./my <create_tables.sql 
./my merc <merc.sql 
./my merc <storage.sql 
