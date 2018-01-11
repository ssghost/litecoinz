FROM debian:jessie

ENV LITECOINZ_CONF=/home/litecoinz/.litecoinz/litecoinz.conf
ENV LITECOINZ_DATA /home/litecoinz/.litecoinz

RUN set -e && \
  apt-get update -q && \
  apt-get install -q -y apt-transport-https wget && \
  wget -qO - https://litecoinz.info/litecoinz.asc | apt-key add - && \
  echo "deb [arch=amd64] https://litecoinz.info/ jessie main" | tee /etc/apt/sources.list.d/litecoinz.list && \
  apt-get update -q && \
  apt-get install litecoinz -q -y && \
  litecoinz-fetch-params && \
  groupadd -r litecoinz && \
  useradd -r -m -g  litecoinz litecoinz && \
  mv /root/.litecoinz-params /home/litecoinz/ && \
  mkdir -p /home/litecoinz/.litecoinz/ && \
  chown -R litecoinz:litecoinz /home/litecoinz && \
  apt-get purge -q -y apt-transport-https wget && \
  apt-get clean -q -y && \
  apt-get autoclean -q -y && \
  apt-get autoremove -q -y && \
  rm -rf "/var/lib/apt/lists/*" "/var/lib/apt/lists/partial/*" "/tmp/*" "/var/tmp/*"

VOLUME /home/litecoinz/.litecoinz

COPY docker-entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]

USER litecoinz

EXPOSE 29332 29333 39332 39333

CMD [ "litecoinzd" ]
