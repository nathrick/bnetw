# GSDK

GSDK is game sdk.

## TBD

* First part is to create asynchronous game server. boost::asio is planned to be used. 

## Useful commands

```bash
docker build -t gsdk .  # build gsdk image
docker run -it gsdk     # run gsdk interactive

docker-compose up
docker-compose up --scale example_client=5
docker-compose down
```