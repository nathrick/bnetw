# bnetw

bnetw is network client-server library based on boost::asio.


## Useful commands

```bash
docker build -t bnetw .  # build bnetw image
docker build -t bnetw . --build-arg TEST=ON  # build bnetw tests
docker run -it bnetw     # run bnetw interactive

docker-compose build
docker-compose up
docker-compose up --scale example_client=5
docker-compose down
```
