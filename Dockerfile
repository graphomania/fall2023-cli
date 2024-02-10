FROM gcc:13.2.0

WORKDIR /app
COPY . /app

RUN apk add --update --no-cache cmake make &&\
    cd build && cmake .. && make

ENTRYPOINT ["./fall2023"]