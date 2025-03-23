# power_control/asgi.py
import os
from django.core.asgi import get_asgi_application
from channels.routing import ProtocolTypeRouter, URLRouter
from channels.auth import AuthMiddlewareStack
import consumer  # Import from the root directory

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'power_control.settings')

application = ProtocolTypeRouter({
    "http": get_asgi_application(),
    "websocket": AuthMiddlewareStack(
        URLRouter(
            consumer.websocket_urlpatterns
        )
    ),
})