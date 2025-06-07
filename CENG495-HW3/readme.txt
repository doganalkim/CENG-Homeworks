PYTHON VENV  commands
- - - - - - 
1 - python3 -m venv ./venv
2 - source ./venv/bin/activate
3 - pip3 install celery redis

- To run redis I have used the following command. My local computer is MacOS. There are alternative ways of  setting up redis. 

redis-stack-server

- To run workers, I have used the following command. The worker name changes for each worker. One can increment the integer to change the name.

celery -A app worker --loglevel=info --concurrency=4 --hostname=worker1@%h

- To run main proram, python3 main.py command can be used.

On my local machine, I also established python virtual environment and run inside it.
IMPORTANT: Filepath is hardcoded for dataset. It may give errors depending on the path. I cannot upload compressed dataset to odtuclass due to file size limit.