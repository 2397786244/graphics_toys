extends MeshInstance


# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass

func _input(event):
	var pos = Vector3(0.0,0.0,0.0);
	# 用WASD 控制方块在四个方向上移动。
	# 用H 控制方向升高，降低
	if event is InputEventKey and event.pressed:
		if event.scancode == KEY_A:
			pos = pos - Vector3(0.5,0.0,0.0);
		elif event.scancode == KEY_D:
			pos = pos + Vector3(0.5,0.0,0.0);
		elif event.scancode == KEY_S:
			pos = pos + Vector3(0.0,0.0,0.5);
		elif event.scancode == KEY_W:
			pos = pos - Vector3(0.0,0.0,0.5);
		elif event.scancode == KEY_H:
			pos = pos + Vector3(0.0,0.5,0.0);
		elif event.scancode == KEY_F:
			pos = pos - Vector3(0.0,0.5,0.0);	
	self.translate(0.25 * pos);
