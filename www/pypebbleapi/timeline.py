import requests
import pprint


class Timeline(object):
    api_root = 'https://timeline-api.getpebble.com'

    error_codes = {
        200: 'Success',
        400: 'The pin object submitted was invalid.',
        403: 'The API key submitted was invalid.',
        410: 'The user token submitted was invalid or does not exist.',
        429: 'Server is sending updates too quickly.',
        503: 'Could not save pin due to a temporary server error.',
    }

    def __init__(self, api_key=None, api_root=None):
        self._api_key = api_key

        if api_root is not None:
            self._api_root = api_root
        else:
            self._api_root = type(self).api_root

    def send_shared_pin(self, topics, pin):
        url = "{}/v1/shared/pins/{}".format(self._api_root, pin['id'])

        result = requests.put(
            url,
            headers={
                'X-API-Key': self._api_key,
                'X-PIN-Topics': ','.join(str(t) for t in topics),
            },
            json=dict(pin),
        )
        pprint.pprint(dict(pin))
        return result

    def send_user_pin(self, user_token, pin):
        url = "{}/v1/user/pins/{}".format(self._api_root, pin['id'])

        result = requests.put(
            url,
            headers={
                'X-User-Token': user_token,
            },
            json=dict(pin),
        )
        pprint.pprint(dict(pin))
        return result

    def delete_user_pin(self, user_token, pin):
        url = "{}/v1/user/pins/{}".format(self._api_root, pin['id'])

        result = requests.delete(
            url,
            headers={
                'X-User-Token': user_token,
            },
        )
        return result

    def subscribe(self, user_token, topic):
        url = "{}/v1/user/subscription/{}".format(self._api_root, topic)

        result = requests.post(
            url,
            headers={
                'X-User-Token': user_token,
            },
        )
        return result

    def unsubscribe(self, user_token, topic):
        url = "{}/v1/user/subscription/{}".format(self._api_root, topic)

        result = requests.delete(
            url,
            headers={
                'X-User-Token': user_token,
            },
        )
        return result
