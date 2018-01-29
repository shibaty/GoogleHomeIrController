# GoogleHomeIrController

## cron

set cron to prevent entering sleep mode.

```sh
*/15 * * * * /usr/bin/curl --silent http://<thisURL>/ &> /dev/null
