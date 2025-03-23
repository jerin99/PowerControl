# consumer.py
from channels.generic.websocket import AsyncWebsocketConsumer
import json

class PowerConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.group_name = "power_group"
        await self.channel_layer.group_add(self.group_name, self.channel_name)
        await self.accept()
        self.units = None
        self.total_power = 0
        print(f"Client connected, channel: {self.channel_name}")

    async def disconnect(self, close_code):
        await self.channel_layer.group_discard(self.group_name, self.channel_name)
        print("Client disconnected")

    async def receive(self, text_data):
        data = json.loads(text_data)
        print("Received:", data)
        
        if 'command' in data:
            if data['command'] in ['on', 'off']:
                print("Sending command:", data['command'])
                await self.channel_layer.group_send(
                    self.group_name,
                    {
                        'type': 'broadcast_message',
                        'message': {'command': data['command']}
                    }
                )
        elif 'units' in data:
            self.units = float(data['units'])
            self.total_power = 0
            print(f"Units set to {self.units}, broadcasting ON")
            await self.channel_layer.group_send(
                self.group_name,
                {
                    'type': 'broadcast_message',
                    'message': {'command': 'on'}
                }
            )
        elif 'power' in data:
            power = float(data['power'])
            self.total_power += power
            print(f"Power: {power}, Total: {self.total_power}")
            await self.channel_layer.group_send(
                self.group_name,
                {
                    'type': 'broadcast_message',
                    'message': {'power': power, 'total': self.total_power}
                }
            )
            if self.units and self.total_power >= self.units:
                print("Limit reached, broadcasting OFF")
                await self.channel_layer.group_send(
                    self.group_name,
                    {
                        'type': 'broadcast_message',
                        'message': {'command': 'off'}
                    }
                )

    async def broadcast_message(self, event):
        await self.send(text_data=json.dumps(event['message']))

from django.urls import re_path
websocket_urlpatterns = [
    re_path(r'ws/power/$', PowerConsumer.as_asgi()),
]