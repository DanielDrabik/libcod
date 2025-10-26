FROM debian:bullseye-slim

RUN dpkg --add-architecture i386 && apt-get update

RUN apt-get install -y --no-install-recommends \
      libstdc++5:i386=1:3.3.6-30 \
      g++-multilib=4:8.3.0-1 \
      default-libmysqlclient-dev:i386=1.0.5 \
      libsqlite3-0:i386=3.27.2-3+deb10u1 \
      libsqlite3-dev:i386=3.27.2-3+deb10u1 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*