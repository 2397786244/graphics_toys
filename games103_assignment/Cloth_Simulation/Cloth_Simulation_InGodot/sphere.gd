extends MeshInstance
class_name Sphere_Obj
var R:float = 1.0;
var c:Vector3;
var move_speed : float = 3.0;
func _ready():
	# 获取球面顶点以及球心。
	c = self.transform.origin;
func _process(delta):
	pass
func _physics_process(delta):
	if Input.is_physical_key_pressed(65):
		self.transform.origin += Vector3(0,0,1) * delta * move_speed;
	if Input.is_physical_key_pressed(68):
		self.transform.origin += Vector3(0,0,-1) * delta * move_speed; 
	if Input.is_physical_key_pressed(83):
		self.transform.origin += Vector3(1,0,0) * delta * move_speed;
	if Input.is_physical_key_pressed(87):
		self.transform.origin += Vector3(-1,0,0) * delta * move_speed;			
