FROM ubuntu:26.04

RUN apt update
RUN apt install -y curl git

RUN curl https://raw.githubusercontent.com/LandonHarter/maylang/refs/heads/master/install.sh | bash
RUN export PATH="$HOME/.may/bin:$PATH"

RUN git clone https://github.com/LandonHarter/maylang.git
RUN cd maylang/docs
CMD may server.may
