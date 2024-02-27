FROM gcc:13.2

WORKDIR /app
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y cmake make
COPY . /app/
RUN mkdir -p build && cd build && rm -r * && cmake .. && make

ENTRYPOINT ["./build/fall2023"]