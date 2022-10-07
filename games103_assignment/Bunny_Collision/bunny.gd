extends MeshInstance

class_name BunnyClass

var VW_Label = Label.new();
	
var Vec3Array = PoolVector3Array();

var Mesh_Data:Array;
var mesh_vertices:Array;
var Orig:Vector3;
var Mass:float = 0.0;
var vm :float = 0.8;
var Bunny_V :Vector3 = Vector3(0,0,0);
var Bunny_W :Vector3 = Vector3(0,0,0);
# 初始角速度、 线速度.
const linear_decay:float = 0.999; # 线速度衰减
const angular_decay:float = 0.98;  # 角速度衰减
const restitution:float = 0.5;  #弹性系数
const firction:float = 0.2;  #摩擦系数
var launched:bool = false;
var Bunny_Default_Position:Vector3 = Vector3(16,10,20);
var I : Basis;
var Identity:Basis;
var Plane1_Pos :Vector3 = Vector3(0,0.1,0);
var Plane2_Pos:Vector3 = Vector3(2,0,0);
var Plane1_Normal:Vector3 = Vector3(0,1,0);
var Plane2_Normal:Vector3 = Vector3(0,0,1);
var Grivity:Vector3 = Vector3(0,-9.8,0);
var IR:Basis;
var q:Quat;
var dt:float = 0.015;

func IR_Prepare(verts:Array,Origin:Vector3)->Basis:
	# mr2 0 0 0
	# 0 mr2 0 0
	# 0 0 mr2 0
	# 0 0 0 1
	var I_Ref:Basis;
	#计算转动惯量.
	var v00:float = 0;
	var v01:float = 0;
	var v02:float = 0;
	var v10:float = 0;
	var v11:float = 0;
	var v12:float = 0;
	var v20:float = 0;
	var v21:float = 0;
	var v22:float = 0;
	Mass = 0.0;
	var diag:float = 0.0;
	for each_v in verts:
		Mass += vm;
		diag = vm * (each_v - Origin).length() * (each_v - Origin).length();
		v00 += diag;
		v11 += diag;
		v22 += diag;
		
		v00 -= vm * (each_v[0] - Origin[0]) * (each_v[0] - Origin[0]);
		v01 -= vm * (each_v[0] - Origin[0]) * (each_v[1] - Origin[1]);
		v02 -= vm * (each_v[0] - Origin[0]) * (each_v[2] - Origin[2]);
		
		v10 -= vm * (each_v[1] - Origin[1]) * (each_v[0] - Origin[0]);
		v11 -= vm * (each_v[1] - Origin[1]) * (each_v[1] - Origin[1]);
		v12 -= vm * (each_v[1] - Origin[1]) * (each_v[2] - Origin[2]);
		
		v20 -= vm * (each_v[2] - Origin[2]) * (each_v[0] - Origin[0]);
		v21 -= vm * (each_v[2] - Origin[2]) * (each_v[1] - Origin[1]);
		v22 -= vm * (each_v[2] - Origin[2]) * (each_v[2] - Origin[2]);
	I_Ref = Basis(Vector3(v00,v10,v20),Vector3(v01,v11,v21),Vector3(v02,v12,v22));	
	return I_Ref;	
func Start():
	# 给兔子一个初速度
	Bunny_V = Vector3(3,8,-25);
	Bunny_W = Vector3(0,2.0,0.0);
func Mat_Diff(M1:Basis,M2:Basis)->Basis:
	var Temp:Basis = M1;
	for i in range(3):
		for j in range(3):
			Temp[j][i] -= M2[j][i];
	return Temp;
func getRotationMatrix(M:Quat)->Basis:
	var x:float = M.x;
	var y:float = M.y;
	var z:float = M.z;
	var w:float = M.w;
	return Basis(Vector3(1 - 2 * y * y- 2 * z * z,2 * x * y - 2 * w * z,
		2 * x * z + 2 * w * y),Vector3(2 * x * y + 2 * w * z,1 - 2 * x * x - 2 * z * z,2 * y * z - 2 * w * x),Vector3(2 * x * z - 2 * w * y,
		2 * y * z + 2 * w * x,1 - 2 * x * x - 2 * y * y));	
# 碰撞检测，更新兔子模型的v和w。P是平面位置。N是平面法线.
func Collision_Impulse(P:Vector3 ,N:Vector3 ):
	var VP:Vector3;
	var xi :Vector3;
	var Rri:Vector3;
	var vi:Vector3;
	var vNi:Vector3;
	var vNi_new:Vector3;
	var vTi:Vector3;
	var vTi_new:Vector3;
	var v_new :Vector3;
	var a:float;
	var pos_sum:Vector3 = Vector3(0,0,0);
	var J :Vector3;
	var K :Basis;
	var Rri_M:Basis;
	var cnt :int = 0;
	var R :Basis;
	var T:Vector3;  # 平移向量
	var ri :Vector3;  # 距离向量
	# 跟兔子模型的每一个顶点进行检查。如果f(x) <= 0，即发生碰撞，那么就更新v、w.
	q = Quat(self.rotation);
	R = getRotationMatrix(q);
	T = self.transform.origin;
	for each_v in mesh_vertices:
		# 计算世界坐标系的位置.
		ri = each_v - Orig;
		xi = T + R * ri;
		VP = xi - P;
		if N.dot(VP) < 0:
			# Step1. 计算顶点速度vi
			Rri = R * ri;
			vi = Bunny_V + Bunny_W.cross(Rri);
			if vi.dot(N) < 0:
				pos_sum += ri;
				cnt += 1;
	# 发生碰撞，计算碰撞的平均位置:
	if cnt > 0:
		ri = pos_sum / cnt;	
		Rri = R * ri; #这里xi是平均碰撞点的位置
		vi = Bunny_V + Bunny_W.cross(Rri);
		var RT:Basis = Basis();
		RT = R.transposed();
		I = R * IR * RT;
		vNi = vi.dot(N)	* N;
		vTi = vi - vNi;
		a = max(1 - firction * (1 + restitution) * vNi.length() / vTi.length(),0.0);
		vNi_new = -1.0 * restitution * vNi;
		vTi_new = a * vTi;
		v_new = vTi_new + vNi_new;
		Rri_M = Basis(Vector3(0,Rri.z,-Rri.y),Vector3(-Rri.z,0,Rri.x),Vector3(Rri.y,-Rri.x,0));
		K = Mat_Diff(Identity.scaled(Vector3(1.0 / Mass,1.0 / Mass,1.0 / Mass)),
		Rri_M * I.inverse() * Rri_M
		);
		J =  K.inverse() * (v_new - vi); # 计算冲量	
		Bunny_V = Bunny_V + 1.0 / Mass * J;
		Bunny_W = Bunny_W + I.inverse() * (Rri_M * J);
		# print(Bunny_W);
	else:
		return;	
# Called when the node enters the scene tree for the first time.
func _ready():
	var Start_Button = Button.new();
	Start_Button.text = "Start!";
	Start_Button.connect("pressed", self, "_button_pressed");
	add_child(Start_Button);
	
	var ReStart_Button = Button.new();
	ReStart_Button.text = "ReStart!";
	ReStart_Button.connect("pressed", self, "_rebutton_pressed");
	ReStart_Button.set_position(Vector2(0,30.0));
	add_child(ReStart_Button);
	
	VW_Label.set_position(Vector2(0,50.0));
	add_child(VW_Label);
	Vec3Array.resize(2);
	
	var msg:String = "Hello Godot!\nHello Bunny! {name}--{time}";
	print(msg.format({"name":"BigDreamer","time":"%.2f" % 12.2}));
	# 返回兔子模型的顶点数组
	Mesh_Data = self.mesh.surface_get_arrays(0);
	# Mesh_Vertics[Mesh.ARRAY_VERTEX] 储存了模型的所有顶点。
	# Mesh.ARRAY_NORMAL 是模型顶点法线的数组.
	mesh_vertices = Mesh_Data[Mesh.ARRAY_VERTEX];
	Orig = Vector3(0,0,0);
	# 计算质心
	for each_v in mesh_vertices:
		Orig += each_v;
	Orig = Orig / mesh_vertices.size();	
	self.global_transform.origin = Bunny_Default_Position;
	
	IR = IR_Prepare(mesh_vertices,Orig);  # 初始化相对参考系的转动惯量矩阵.
	
	# test
# Called every frame. 'delta' is the elapsed time since the previous frame.
# warning-ignore:unused_argument
func _process(delta):
	
	if launched == true:
	# Step1 更新速度
		# 计算重力，考虑空气摩擦力造成的速度衰减。
		Bunny_V += Grivity * dt;
		Bunny_V *= linear_decay;
		Bunny_W *= angular_decay;
	# Step2 Collision Impulse
		Collision_Impulse(Plane1_Pos,Plane1_Normal);
		Collision_Impulse(Plane2_Pos,Plane2_Normal);
		Vec3Array.set(0,Bunny_V);
		Vec3Array.set(1,Bunny_W);
		# Vec3Array.set(2,self.rotation);
		
		VW_Label.text = String(Vec3Array);
	# Step3 更新位置和朝向
	# 3.1 线性状态
		var x:Vector3 = self.global_transform.origin;
		
		x += dt * Bunny_V; 
	# 3.2 角度状态
		q = Quat(self.rotation);
		# var rq:Quat = Quat(Bunny_W.normalized(),deg2rad(delta * Bunny_W.length()));  # 根据w和 delta计算得到旋转角度
		# var dw : Vector3 = 0.5 * dt * Bunny_W;
		var rq:Quat = Quat(Bunny_W.normalized(),deg2rad(80 * dt * Bunny_W.length()));
		q = rq.normalized() * q;
	# 将状态赋值物体
		self.global_transform.origin = x;
		self.rotation = q.get_euler();
func _button_pressed():
	if launched == false:
		launched = true;
		
		Start();
		print("Bunny Shooting!");

func _rebutton_pressed():
	if launched == true:
		launched = false;
		#位置重置
		self.transform.origin = Bunny_Default_Position;
		self.rotation = Vector3(0,0,0);
		print("Bunny ReStart");
