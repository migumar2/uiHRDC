# UBUNTU IMAGES @ DOCKERHUB https://hub.docker.com/_/ubuntu/
# WE will use ubuntu xenial 16.04 version.

############################################################################

# Set the base image to Ubuntu
# --- In our experimental framework we used ubuntu 14.04-64bit
# https://hub.docker.com/_/ubuntu/
#
  ##ubuntu 14.04
FROM ubuntu:trusty   

	##ubuntu 16
#FROM ubuntu:xenial  

#FROM ubuntu
#FROM gcc:4.8

############################################################################

# File Author / Maintainer
MAINTAINER Miguel A. Martínez-Prieto & Antonio Fariña

########################################################################
# Installs required software:

RUN apt-get update && apt-get install -y \
	cmake \
	gcc-multilib \
	g++-multilib \
	libboost-all-dev \
	p7zip-full \
	screen \
	texlive-latex-base \
	texlive-fonts-recommended \
	ghostscript \
	gnuplot-qt \	
	openssh-server

	#texlive-latex-extra
	#sudo apt-get install texlive-fonts-recommended 


########################################################################
# permits us to connect to SSH/SFTP port #22

RUN mkdir /var/run/sshd

RUN echo 'root:screencast' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin without-password/' /etc/ssh/sshd_config

## SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]

########################################################################
##### Creates user 'user' with password 'userR1'
	#RUN useradd --create-home --skel /etc/skel -p user1 --shel /bin/bash user

RUN adduser user --gecos "First Lst, RoomNumber, WorkPhone, HomePhone" --disabled-password
RUN adduser user sudo

###password is set below (once github repository is copied into /home/user/github!)
########################################################################
		

########################################################################
# CLONES / COPIES github repository

RUN cd /home/user 

#Copies local github repository
COPY github /home/user/github

# Clone git repository
#RUN cd /home/user 
#RUN cd home && git clone https://github.com/migumar2/uiHRDC.git
#RUN git clone https://github.com/migumar2/uiHRDC.git

########################################################################
# Installs snappy-1.1.1
	RUN mkdir toInstall 
	RUN cp /home/user/github/indexes/NOPOS/EliasFano.OV14/partitioned_elias_fano/snappy-1.1.1.tar.gz toInstall/snappy-1.1.1.tar.gz
		#COPY snappy-1.1.1.tar.gz toInstall/snappy-1.1.1.tar.gz
	RUN  cd toInstall && tar xzvf snappy-1.1.1.tar.gz && cd snappy-1.1.1 && ./configure && make install 
	RUN rm -rf /toInstall



########################################################################
#Sets user 'user' password 'userR1'

RUN cd /home/user/github/docker && sh setpassword.sh

	#COPY setpassword.sh setpassword.sh
	#COPY passwords.txt  passwords.txt

########################################################################
# Grants permissions for user in its home folder 
RUN chown user:user -R /home/user/


########################################################################
# configures -screen- manager terminal emulator
	RUN cp /home/user/github/docker/screenrc /root/.screenrc
	RUN cp /home/user/github/docker/screenrc /home/user/.screenrc
	RUN chown user:user /home/user/.screenrc



########################################################################
# Shows information regarding configuration of the image/container
## Shows Ubutu release.
#RUN lsb_release -a  ##works on ubuntu 14, not on 16
#RUN gcc --version
#RUN g++ --version
#RUN cmake --version


########################################################################
# shows final welcome message and helpful information

RUN cat /home/user/github/docker/final.docker.message.txt

########################################################################

