FROM python:3.9

RUN apt-get update && apt-get install -y --no-install-recommends build-essential

COPY . /app
WORKDIR /app

RUN python -m build

# The resulting wheels are in the /app/dist directory, which is captured by the CI script
