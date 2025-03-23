from django.urls import path
from . import views

urlpatterns = [
    path('', views.control, name='control'),
    path('set_units/', views.set_units, name='set_units'),
    path('monitor/', views.monitor, name='monitor'),
]