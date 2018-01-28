# WebAPI Reference

## Request

### path
http://{host}/ctl

### method
PUSH

### Content-Type
application/json

### Request-Body

| key     | value                |
| --------|:---------------------|
| target  | tv/light/ac          |
| action  | poweron/poweroff     |
|         | (tv_only)volumeup/volumedown/channelup/channeldown |
|         | (ac_only)heateron,acon |

#### Example
```json
{
  "target": "tv"
  "action": "poweron"
}
