This tutorial uses the I2RS Hackathon VM:
https://github.com/i2rs-wg/hackathon-code/releases/download/v2.0/i2rs_quagga_working3.ova

1. Remove proxy configuration:
unset http_proxy
unset https_proxy
unset ftp_proxy
unset FTP_PROXY
unset HTTP_PROXY
unset HTTPS_PROXY

2. Create work folder:
mkdir quagga-i2rs
cd quagga-i2rs

3. Remove installed Quagga
sudo apt-get purge --auto-remove quagga
sudo apt-get purge --auto-removequagga 

4. Delete manually installed library
sudo rm /usr/local/lib/libospf*
sudo rm /usr/local/lib/libzebra*
sudo rm -rf /usr/lib/quagga

5. Get Quagga source code for Ubuntu:
wget --no-proxy https://launchpad.net/ubuntu/+archive/primary/+files/quagga_1.0.20160315-1.dsc
wget --no-proxy https://launchpad.net/ubuntu/+archive/primary/+files/quagga_1.0.20160315.orig.tar.xz
wget --no-proxy https://launchpad.net/ubuntu/+archive/primary/+files/quagga_1.0.20160315-1.debian.tar.xz

6. Extract the sources:
dpkg-source -x quagga_1.0.20160315-1.dsc

7. Install Quagga and compilation requirements:
sudo apt-get update
sudo apt-get install libreadline-dev libreadline6-dev devscripts texlive-fonts-recommended dh-autoreconf
sudo apt-get build-dep quagga

8. Get and extract the files from the Github
Get i2rs-source from Github (i2rs-hackathon/quagga-i2rsd/i2rs-source)

9. Replace base files and add I2RS files:
cp -r i2rs-source/* quagga-1.0.20160315/
cd quagga-1.0.20160315/

10. Build the .deb package
debuild -rfakeroot -us -uc -b -nc

11. Install the .deb
cd ..
sudo dpkg -i quagga_1.0.20160315-1_amd64.deb quagga-dbg_1.0.20160315-1_amd64.deb quagga-doc_1.0.20160315-1_all.deb

12. Run your network 
The example ospf_bgp (updated version on Github, not the one in the i2rs_quagga_working3.ova) 
starts the i2rsd on router r010_1

13. Clean previous compilation and installation to recompile
sudo apt-get remove quagga
sudo apt-get purge quagga
cd quagga-i2rs/quagga-1.0.20160315/
debuild -rfakeroot -us -uc -b -nc -Tclean

14. Change the code
If new files are added, don't forget to change the Makefile.am 
GO TO STEP 10 