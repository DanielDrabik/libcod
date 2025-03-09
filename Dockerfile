FROM ddrabik/libcod-build-environment:45e2b59

WORKDIR /app

COPY . .

RUN chmod +x doit.sh

RUN ./doit.sh cod2_1_0
RUN ./doit.sh cod2_1_2
RUN ./doit.sh cod2_1_3

CMD ["bash"]