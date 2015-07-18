from six import iteritems


class _DictionableObject(object):
    def __iter__(self):
        for k, v in iteritems(self.__dict__):
            if k.startswith('_'):
                continue
            yield k, v

    def __getitem__(self, key):
        return self.__dict__[key]

    def __setitem__(self, key, value):
        self.__dict__[key] = value

    def __delitem__(self, key):
        del self.__dict__[key]


class Pin(_DictionableObject):
    icon = {
        'BASEBALL': 'system://images/TIMELINE_BASEBALL_TINY',
        'CHAT': 'system://images/TIMELINE_CHAT_TINY',
        'TAPE': 'system://images/TIMELINE_TAPE_TINY',
        'FOOTBALL': 'system://images/TIMELINE_FOOTBALL_TINY',
        'MAIL': 'system://images/TIMELINE_MAIL_TINY',
        'BULB': 'system://images/TIMELINE_BULB_TINY',
        'CALENDAR': 'system://images/TIMELINE_CALENDAR_TINY',
        'SUN': 'system://images/TIMELINE_SUN_TINY',
        'PIN': 'system://images/TIMELINE_PIN_TINY',
        'BATT_FULL': 'system://images/TIMELINE_BATT_FULL_TINY',
        'BATT_EMPTY': 'system://images/TIMELINE_BATT_EMPTY_TINY',
        'ALARM': 'system://images/TIMELINE_ALARM_TINY',
    }

    layout_type = {
        'GENERIC_PIN': 'genericPin',
        'CALENDAR_PIN': 'calendarPin',
        'GENERIC_REMINDER': 'genericReminder',
        'GENERIC_NOTIFICATION': 'genericNotification',
        'COMM_NOTIFICATION': 'commNotification',
        'WEATHER_PIN': 'weatherPin',
        'SPORTS_PIN': 'sportsPin',
    }

    action_type = {
        'OPEN_WATCH_APP': 'openWatchApp',
    }

    def __init__(self,
            id,
            time,
            duration=None,
            createNotification=None,
            updateNotification=None,
            layout=None,
            reminders=None,
            actions=None,
            ):
            self.id = id
            self.time = time

            if duration is not None:
                self.duration = duration

            if createNotification is not None:
                self.createNotification = createNotification

            if updateNotification is not None:
                self.updateNotification = updateNotification

            if layout is not None:
                self.layout = layout

            if reminders is not None:
                self.reminders = reminders

            if actions is not None:
                self.actions = actions

    def __iter__(self):
        for k, v in iteritems(self.__dict__):
            if k.startswith('_'):
                continue
            yield k, v

    def __getitem__(self, key):
        return self.__dict__[key]
