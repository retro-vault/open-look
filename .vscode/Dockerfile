FROM wischner/gcc-x86_64-linux-x11:latest
USER root
RUN apt-get update && apt-get install -y --no-install-recommends \
    libnss3 libnspr4 libatk1.0-0 libatk-bridge2.0-0 libatspi2.0-0 \
    libxcomposite1 libxdamage1 libxkbcommon0 libasound2 libgbm1 libcups2 \
    libgtk-3-0 fonts-dejavu-core xdotool imagemagick \
    && rm -rf /var/lib/apt/lists/*
