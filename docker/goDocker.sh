
#avoids the need for using "sudo" before docker commands
sudo usermod -aG docker $USER

#Builds docker image named "repet" using dockerfile
sudo docker build -t repet --rm=true . 

# Creates a container named "repet-exp"
# sudo docker run --name repet-exp -it repet
# sudo docker run --name repet-exp --memory="32g" --memory-swap="32g"  -p 22222:22 -it repet 
 sudo docker run --name repet-exp -p 22222:22 -it repet 
 
 #** NOW press CTRL+P CTRL+Q to detach from docker-container  (if connected) !!!!!!!!!!!!!!!!!!!!!!!!!!1
 
# If stopped, starts the container repet-exp (that was already created)
#sudo docker start repet-exp
echo "to detach from docker session --> CTRL+P CTRL+Q"
sudo docker attach repet-exp



# sudo docker stop repet-exp       #stops container  
# sudo docker rm repet-exp         #remove container  
# sudo docker rmi repet            #remove image 
# sudo docker rmi ubuntu:trusty    #remove image 
# sudo docker images               ##see images
# sudo docker ps                   ##see active container
# sudo docker start/stop repet-exp ##starts/stop container


#there is an user 'user' (with sudo capabilities). His password is 'user1'
# you can connect to docker machine as:
 ssh user@localhost -p 22222 
#     enter password 'user1' when prompted
#

#**
#** We found issues when running scripts within the docker container with a
#** non-root user, so we recommend to:
#**   - connect by ssh with user 'user': $ ssh user@localhost -p 22222  <pass userR1>
#**   - then become root: $ sudo su                                     <pass userR1>
#**   -   now open screen-virtual-terminal console if you want
#**   - finally if you want to retrieve all the data in benchmark folder:
#**         chown user:user /home/user -R
#**         or tar czvf /home/user/benchmark.tar.gz /home/user/github/benchmark
#**
