#!/bin/bash

docker run --rm -it \
  -v $(pwd):/app \
  -w /app \
  ddrabik/libcod-build-environment:45e2b59 \
  bash -c "chmod +x doit.sh && ./doit.sh cod2_1_0 && ./doit.sh cod2_1_2 && ./doit.sh cod2_1_3"