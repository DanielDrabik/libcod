FROM ubuntu:20.04

RUN dpkg --add-architecture i386 && apt-get update

RUN apt-get install -y \
    build-essential \
    g++-multilib \
    libmysqlclient-dev:i386 \
    libsqlite3-dev:i386

WORKDIR /app

COPY . .

RUN chmod +x doit.sh

RUN ./doit.sh cod2_1_0
RUN ./doit.sh cod2_1_2
RUN ./doit.sh cod2_1_3

CMD ["bash"]