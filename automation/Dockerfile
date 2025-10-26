FROM debian:bullseye-slim

RUN dpkg --add-architecture i386 && apt-get update

RUN apt-get install -y --no-install-recommends \
      g++-multilib \
      libstdc++6:i386 \
      default-libmysqlclient-dev:i386 \
      libsqlite3-0:i386 \
      libsqlite3-dev:i386 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*