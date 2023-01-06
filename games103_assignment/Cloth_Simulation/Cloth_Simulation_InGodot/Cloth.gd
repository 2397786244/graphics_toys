extends MeshInstance
var Vertices_List:Array;
var E : Array; # 三角形储存
var V : Array;  # 储存速度
var L:Array;  # 储存每条边的原始长度
const t:float = 0.0333;
var mass:float = 1.0;
var damping:float = 0.999;
var spring_k :float = 8000.0;
const vn:int = 8;
var g:Vector3 = Vector3(0,-9.8,0);
var r:float = 0.5;
var open:bool = true;
func _ready():
	# 储存三角形信息。去重。
	Vertices_List = self.mesh.surface_get_arrays(0)[Mesh.ARRAY_VERTEX];
	var Triangles:Array;  # 将正方形网格划分为三角形。Triangles记录2 * (n-1)**2 个三角形
	# 总共6 (n-1)**2个顶点index。
	Triangles.resize((vn-1) * (vn-1) * 6);
	var e:int = 0;
	for j in range(0,vn-1,1):
		for i in range(0,vn-1,1):
			Triangles[e * 6 + 0] = j * vn + i;
			Triangles[e * 6 + 1] = j * vn + i + 1;
			Triangles[e * 6 + 2] = (j+1) * vn + i + 1;
			Triangles[e * 6 + 3] = j * vn + i;
			Triangles[e * 6 + 4] = (j+1) * vn + i + 1;
			Triangles[e * 6 + 5] = (j+1) * vn + i;
			e = e + 1;
	var _E:Array;
	_E.resize(Triangles.size() * 2);
	for i in range(0,Triangles.size(),3):
		_E[2 * i + 0] = Triangles[i+0];
		_E[2 * i + 1] = Triangles[i+1];
		_E[2 * i + 2] = Triangles[i+1];
		_E[2 * i + 3] = Triangles[i+2];
		_E[2 * i + 4] = Triangles[i+2];
		_E[2 * i + 5] = Triangles[i+0];
	for i in range(0,_E.size(),2):
		if(_E[i] > _E[i+1]):
			# 以上两数做交换。
			var temp = _E[i];
			_E[i] = _E[i+1];
			_E[i+1] = temp;
	quick_sort(_E,0,_E.size() / 2 - 1);
	var e_number:int = 0;					
	for i in range(0,_E.size(),2):
		if(i == 0 || _E[i + 0] != _E[i - 2] || _E[i + 1] != _E[i - 1]):
			e_number = e_number + 1;
			
	E.resize(e_number * 2);
	e = 0;
	for i in range(0,_E.size(),2):
		if(i == 0 || _E[i+0] != _E[i-2] || _E[i + 1] != _E[i - 1]):
			E[2 * e + 0] = _E[i + 0];
			E[2 * e + 1] = _E[i + 1];
			e = e + 1;
	L.resize(E.size() / 2);
	for i in range(0,E.size() / 2):
		var v0:int = E[2 * i + 0];
		var v1:int = E[2 * i + 1];
		L[i] = (Vertices_List[v0] - Vertices_List[v1]).length();
	V.resize(Vertices_List.size());
	V.fill(Vector3(0,0,0));
	
		
# 布料 和球碰撞检测.
# XA: Mesh顶点数组
func Collision_Detect(XA:Array):
	var Sphere:Sphere_Obj = get_node(@"../sphere");  # 获取Spatial下面的兄弟节点。
	var Sphere_Pos = Sphere.transform.origin;
	var _m = ArrayMesh.new();
	_m.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES,self.mesh.surface_get_arrays(0));
	var MDT = MeshDataTool.new();
	MDT.create_from_surface(_m,0);
	for j in range(MDT.get_vertex_count()):
		MDT.set_vertex(j,XA[j]);
	for i in range(XA.size()):
		if (i == 0 || i == vn-1):
			continue;
		# 获取布料表面顶点。 
		var X:Vector3 = XA[i];
		# 拿World_Pos和球心距离进行判断。
		if((X - Sphere_Pos).length() < r):
			# 冲量法更新位置和速度.
			V[i] = V[i] + 1.0 / t * (Sphere_Pos + r * (XA[i] - Sphere_Pos) / (XA[i] - Sphere_Pos).length() - XA[i]);
			XA[i] = Sphere_Pos + r * (XA[i] - Sphere_Pos) / (XA[i] - Sphere_Pos).length();
			MDT.set_vertex(i,XA[i]);
	_m.surface_remove(0);
	MDT.commit_to_surface(_m);
	self.mesh = _m;
# 梯度计算
func get_Gradient(X:Array,X_hat:Array,t:float,G:Array):
	var j :int;
	var i :int;
	for k in range(X.size()):
		G[k] = 1.0 / (t * t) * mass * (X[k] - X_hat[k]) - mass * g;
	for k in range(0,E.size(),2):
		# 遍历每一条边，然后计算每条边两个顶点的gi
		i = E[k];
		j = E[k+1];
		G[i] = G[i] + spring_k * (1.0 - L[k / 2] / (X[i] - X[j]).length()) * (X[i] - X[j]);
		G[j] = G[j] - spring_k * (1.0 - L[k / 2] / (X[i] - X[j]).length()) * (X[i] - X[j]);


func _process(delta):
	if(open == true):
		#首先获取顶点位置
		var X:Array;
		X.resize(Vertices_List.size());
		var _m = ArrayMesh.new();
		_m.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES,self.mesh.surface_get_arrays(0));
		
		var MDT1 = MeshDataTool.new();
		MDT1.create_from_surface(_m,0);
		for i in range(MDT1.get_vertex_count()):
			X[i] = MDT1.get_vertex(i);
			
		var X_hat:Array;
		X_hat.resize(Vertices_List.size());
		X_hat.fill(Vector3(0,0,0));
		
		var G:Array;
		G.resize(Vertices_List.size());
		G.fill(Vector3(0,0,0));
		
		# initial 
		for i in range(X.size()):
			V[i] = damping * V[i];
			X_hat[i] = X[i] + t * V[i];
		X = X_hat;	
		# calc Gradient
		for i in range(0,32):
			get_Gradient(X,X_hat,t,G);
			for j in range(X.size()):
				if( j == 0 || j == vn-1):
					continue;  # 两个固定点不更新位置。
				X[j] = X[j] - 1.0 / (1.0 / (t * t) * mass + 4.0 * spring_k) * G[j];
				# update X
			
		# update:velocity
		for i in range(X.size()):
			V[i] = V[i] + (X[i] - X_hat[i]) / t;	
		# update: X
		var MDT = MeshDataTool.new();
		MDT.create_from_surface(_m,0);
		for i in range(MDT.get_vertex_count()):
			MDT.set_vertex(i,X[i]);
		_m.surface_remove(0);
		MDT.commit_to_surface(_m);
		self.mesh=_m;
		
		Collision_Detect(X);
		
		# recalc normals
		_m = ArrayMesh.new();
		_m.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES,self.mesh.surface_get_arrays(0));
		var mdt = MeshDataTool.new();
		mdt.create_from_surface(_m,0);
		
		for i in range(mdt.get_face_count()):
			var a = mdt.get_face_vertex(i,0);
			var b = mdt.get_face_vertex(i,1);
			var c = mdt.get_face_vertex(i,2);
			var ap = mdt.get_vertex(a);
			var bp = mdt.get_vertex(b);
			var cp = mdt.get_vertex(c);
				
			var n = (bp - cp).cross(ap - cp).normalized();
			mdt.set_vertex_normal(a, (n + mdt.get_vertex_normal(a)).normalized());
			mdt.set_vertex_normal(b, (n + mdt.get_vertex_normal(b)).normalized());
			mdt.set_vertex_normal(c, (n + mdt.get_vertex_normal(c)).normalized());
		_m.surface_remove(0);
		mdt.commit_to_surface(_m);	
		self.mesh = _m;	
func quick_sort(_E:Array,l:int,r:int):
	var j:int;
	if(l < r):
		j = quick_sort_partition(_E,l,r);
		quick_sort(_E,l,j-1);
		quick_sort(_E,j+1,r);
func quick_sort_partition(E:Array,l:int,r:int)->int:
	var pivot_0:int;
	var pivot_1:int;
	var i:int;
	var j:int;
	pivot_0 = E[l * 2 + 0];
	pivot_1 = E[l * 2 + 1];
	i = l;
	j = r + 1;
	while (true):
		i = i + 1;
		while(i <= r && (E[i * 2]< pivot_0 || E[i * 2] == pivot_0 && E[i * 2 + 1] <= pivot_1)):
			i = i + 1;
		j = j - 1;
		while(E[j * 2] > pivot_0 || E[j * 2] == pivot_0 && E[j * 2 + 1] > pivot_1):
			j = j - 1;			
		if(i >= j):
			break;
		var temp = E[2 * i];
		E[2 * i] = E[2 * j];
		E[2 * j] = temp;
		temp = E[2 * i + 1];
		E[2 * i + 1] = E[2 * j + 1];
		E[2 * j + 1] = temp;
		# Swap 
	var temp = E[2 * l + 0];
	E[2 * l + 0] = E[2 * j + 0];
	E[2 * j + 0] = temp;
	temp = E[2 * l + 1];
	E[2 * l + 1] = E[2 * j + 1];
	E[2 * j + 1] = temp;
	return j;	

