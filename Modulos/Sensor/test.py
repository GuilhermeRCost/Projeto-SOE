from gpiozero import MotionSensor

pir = MotionSensor(11)

while True:
	pir.wait_for_motion()
	print("You moved")
	pir.wait_for_no_motion()