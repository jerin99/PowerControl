# power_control/consumers.py
from channels.generic.websocket import AsyncWebsocketConsumer
import json

# Shared state (for simplicity; use cache in production)
shared_state = {
    'units': None,
    'total_power': 0.0
}

class PowerConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.group_name = "power_group"
        await self.channel_layer.group_add(self.group_name, self.channel_name)
        await self.accept()
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
                if data['command'] == 'on':
                    shared_state['total_power'] = 0  # Reset total when turning ON
                await self.channel_layer.group_send(
                    self.group_name,
                    {
                        'type': 'broadcast_message',
                        'message': {'command': data['command']}
                    }
                )
        elif 'units' in data:
            shared_state['units'] = float(data['units'])
            shared_state['total_power'] = 0  # Reset total when setting units
            print(f"Units set to {shared_state['units']}, Total reset to {shared_state['total_power']}")
            await self.channel_layer.group_send(
                self.group_name,
                {
                    'type': 'broadcast_message',
                    'message': {'command': 'on'}  # Start when units are set
                }
            )
        elif 'power' in data:
            power = float(data['power'])
            shared_state['total_power'] += power
            print(f"Power: {power}, Total: {shared_state['total_power']}, Units limit: {shared_state['units']}")
            # Send update to monitor
            await self.channel_layer.group_send(
                self.group_name,
                {
                    'type': 'broadcast_message',
                    'message': {
                        'power': power,
                        'total': shared_state['total_power'],
                        'status': 'Running' if shared_state['units'] is None or shared_state['total_power'] < shared_state['units'] else 'Stopped'
                    }
                }
            )
            # Check if total power exceeds or equals the set units
            if shared_state['units'] is not None and shared_state['total_power'] >= shared_state['units']:
                print(f"Limit {shared_state['units']} reached or exceeded (Total: {shared_state['total_power']}), sending OFF")
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