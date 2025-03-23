from django.shortcuts import render

def control(request):
    return render(request, 'control.html')

def set_units(request):
    return render(request, 'set_units.html')

def monitor(request):
    return render(request, 'monitor.html')