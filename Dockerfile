FROM debian:10 AS builder
RUN dpkg --add-architecture i386 && \
	apt-get update -y && \
	apt-get install -y --no-install-recommends \
		build-essential=12.6 \
		zlib1g:i386=1:1.2.11.dfsg-1+deb10u2 \
		default-libmysqlclient-dev:i386=1.0.5 \
		gcc-multilib=4:8.3.0-1 \
		cmake=3.13.4-1 && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/* && \
    mkdir -p /opt/var/astonia/zones/generic && \
    mkdir -p /opt/var/astonia/.obj
COPY --link ./Makefile /opt/var/astonia/Makefile
COPY --link ./src /opt/var/astonia/src
WORKDIR /opt/var/astonia
RUN make

FROM debian:10 AS runner
RUN dpkg --add-architecture i386 && \
	apt-get update -y && \
	apt-get install -y --no-install-recommends \
		zlib1g:i386=1:1.2.11.dfsg-1+deb10u2 \
		default-libmysqlclient-dev:i386=1.0.5 \
		gcc-multilib=4:8.3.0-1  && \
	apt-get clean && \
	rm -rf /var/lib/apt/lists/*
RUN mkdir -p /opt/var/astonia/zones/generic
WORKDIR /opt/var/astonia
COPY --from=builder /opt/var/astonia/update_server .
COPY --from=builder /opt/var/astonia/chatserver .
COPY --from=builder /opt/var/astonia/create_* .
COPY --from=builder /opt/var/astonia/server .
COPY --from=builder /opt/var/astonia/runtime .
COPY --from=builder /opt/var/astonia/zones/generic /opt/var/astonia/zones/generic
VOLUME /opt/var/astonia/zones
ENTRYPOINT ["/opt/var/astonia/server"]