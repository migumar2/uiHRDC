#avoids the need for using "sudo" before docker commands
#sudo usermod -aG docker fari

#remove all docker containers 
# sudo docker rm $(sudo docker ps -a -f status=exited -q)

#remove all docker images
# sudo docker rmi $(sudo docker images -a -q)



sudo docker stop repet-exp
sudo docker rm repet-exp
sudo docker rmi repet
sudo docker rmi ubuntu:trusty
