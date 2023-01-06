extends MeshInstance

var size:int=70;
var rate:float = 0.005;
var gamma:float = 0.004;
var damping:float =0.996;
var old_h:Array;
var low_h:Array;
var vh:Array;
var b:Array;

var cg_mask:Array;  # bool
var cg_p:Array;
var cg_r:Array;
var cg_Ap:Array;
var tag:bool=true;

var Box:MeshInstance;

# Called when the node enters the scene tree for the first time.

func _ready():
	randomize();
	Box = get_node("/root/3D/Box");
	var mesh = self.mesh;
	var face_array = mesh.surface_get_arrays(0);
	var X = face_array[0];
	
	low_h.resize(size*size);
	old_h.resize(size*size);
	vh.resize(size*size);
	b.resize(size*size);
	
	cg_mask.resize(size*size);
	cg_Ap.resize(size*size);
	cg_p.resize(size*size);
	cg_r.resize(size*size);
	for i in range(0,size):
		for j in range(0,size):
			low_h[i*size+j]=9999.9;
			old_h[i*size+j] = 0.0;
			vh[i*size+j] = 0.0;
			b[i*size+j]=0;
			cg_mask[i*size+j]=false;
			cg_Ap[i*size+j]=0;
			cg_p[i*size+j]=0;
			cg_r[i*size+j]=0;
func A_Times(mask:Array,x:Array,Ax:Array,li:int,ui:int,lj:int,uj:int):
	for i in range(li,ui+1):
		for j in range(lj,uj+1):
			if i >= 0 && j >=0 && i < size && j < size && (mask[i*size+j] == true):
				Ax[i*size+j]=0;
				if i != 0:
					Ax[i*size+j]-= x[(i-1)*size+j]-x[i*size+j];
				if i != size - 1:
					Ax[i*size + j]-=x[(i+1)*size+j]-x[i*size+j];	
				if j != 0:
					Ax[i*size+j]-=x[i*size+j-1]-x[i*size+j];
				if j != size - 1:
					Ax[i*size+j]-=x[i*size+j+1]-x[i*size+j];
						
func Dot(mask:Array,x:Array,y:Array,li:int,ui:int,lj:int,uj:int) -> float:
	var ret:float = 0.0;
	for i in range(li,ui+1):
		for j in range(lj,uj+1):
			if i>=0 && j>=0 && i<size && j<size && (mask[i*size+j]==true) :
				ret += x[i*size+j] * y[i*size+j];
	return ret;
func Conjugate_Gradient(mask:Array,b:Array,x:Array,li:int,ui:int,lj:int,uj:int):
	A_Times(mask, x, cg_r, li, ui, lj, uj);
	for i in range(li,ui+1):
		for j in range(lj,uj+1):
			if i>=0 && j>=0 && i<size && j<size && (mask[i*size+j]==true):
				cg_r[i*size+j] = b[i*size+j] - cg_r[i*size+j];
				cg_p[i*size+j] = cg_r[i*size+j];
	var rk_norm:float = Dot(mask, cg_r, cg_r, li, ui, lj, uj);			
	for k in range(0,128):
		if rk_norm < 1e-10:
			break;
		A_Times(mask, cg_p, cg_Ap, li, ui, lj, uj);
		var alpha:float =rk_norm/Dot(mask, cg_p, cg_Ap, li, ui, lj, uj);
		for i in range(li,ui+1):
			for j in range(lj,uj+1):
				if i>=0 && j>=0 && i<size && j<size && (mask[i*size+j] == true):
					x[i*size+j] += alpha * cg_p[i*size+j];
					cg_r[i*size+j] -= alpha * cg_Ap[i*size+j];
		var _rk_norm:float = Dot(mask, cg_r, cg_r, li, ui, lj, uj);
		var beta:float =_rk_norm/rk_norm;
		rk_norm=_rk_norm;
		for i in range(li,ui+1):
			for j in range(lj,uj+1):
				if i>=0 && j>=0 && i<size && j<size && (mask[i*size+j] == true):
					cg_p[i*size+j]	= cg_r[i*size+j]+beta*cg_p[i*size+j];
											
func Block2WaterCoupling(Box:MeshInstance,new_h:Array,X:Array):
	var bottom:float = -2.0;
	var bounds = Box.get_aabb();
	var minv:Vector3 = bounds.position;
	var maxv:Vector3 = bounds.end;
	var minx = minv.x;
	var  minz = minv.z;
	var maxx = maxv.x;
	var maxz = maxv.z;
	var li:int = int(max((minx-1.0)/0.1 + size / 2.0 ,0.0));
	var ui:int = int(min((maxx + 1.0) / 0.1 + size / 2.0,size - 1.0));
	var lj:int = int(max((minz-1.0)/0.1+size/2.0,0.0));
	var uj:int = int(min((maxz+1.0)/0.1+size/2.0,size-1.0));
	var box_pos = Box.translation;
	var box_left:float = box_pos.x-0.25;
	var box_right:float = box_pos.x+0.25;
	var box_upper:float = box_pos.z-0.25;
	var box_lower:float = box_pos.z+0.25;
	for i in range(0,size):
		for j in range(0,size):
			cg_mask[i*size+j] = false;
			if i <= ui && i >= li && j <= uj && j >= lj:
				b[i*size + j] = 0.0;
				var depth:float;
				depth = box_pos.y - 0.25;
				if depth < 0.0 && X[i*size+j].x >= box_left && X[i*size+j].x <= box_right && X[i*size+j].z >= box_upper && X[i*size+j].z <= box_lower:
					cg_mask[i*size+j]=true;
					low_h[i*size+j]=depth;
					b[i*size+j] = 1.0 / rate * (new_h[i*size+j] - low_h[i*size+j]);
						
	Conjugate_Gradient(cg_mask, b,vh, li, ui, lj, uj);	
					
func Shallow_Wave(h:Array,new_h:Array):
	var left:int;
	var right:int;
	var upper:int;
	var lower:int;
	
	for i in range(0,size):
		left = max(0,i-1);
		right = min(size-1,i+1);
		
		for j in range(0,size):
			upper = max(0,j-1);
			lower = min(size-1,j+1);
			new_h[i * size + j] = h[i *size+j]  + damping * (h[i*size+j] - old_h[i*size+j]) + rate * (h[left*size+j] + h[right*size+j] + h[i*size+upper] + h[i*size + lower] - 4 * h[i*size+j]);
	var face_array = mesh.surface_get_arrays(0);
	var X = face_array[0];		
	
	Block2WaterCoupling(Box,new_h,X);
				
	for i in range(0,size):
		for j in range(0,size):
			vh[i*size+j] *= gamma;
	for i in range(0,size):
		lower = max(0,i-1);
		upper = min(size-1,i+1);
		for j in range(0,size):
			left = max(0,j-1);
			right = min(size-1,j+1);
			new_h[i*size+j] += rate * (vh[i*size+left]+vh[i*size+right]+vh[upper*size+j]+vh[lower*size+j]-4*vh[i*size+j]);
			
	#old_h <- h;h <- new_h
	for i in range(0,size):
		for j in range(0,size):
			old_h[i*size+j] = h[i*size+j];
			h[i*size+j] = new_h[i*size+j];
		
# Called every frame. 'delta' is the elapsed time since the previous frame.
	
func _process(delta):
	var mesh = self.mesh;
	var face_array = mesh.surface_get_arrays(0);
	var X = face_array[0];
	
	var new_h:Array;
	var h:Array;
	h.resize(X.size());
	new_h.resize(X.size());
	# Load X.y to h
	for i in range(0,size):
		for j in range(0,size):
			h[i*size+j]=X[i*size+j].y;
			
	# 按下PageDown的时候，随机位置的顶点增加其高度
	if Input.is_action_pressed("ui_page_down"):
		var i :int = randi() % size;
		var j :int = randi() % size;
		
		var v:float = randf() * 0.1 + 0.02;
		h[i*size+j] += v;
		var left:int = max(0,i-1);
		var right:int = min(size-1,i+1);
		var top:int = max(j-1,0);
		var bottom:int = min(j+1,size-1);
		
		h[left*size+j]-= v / 8.0;
		h[right*size+j]-=v/8.0;
		h[i*size+top]-=v/8.0;
		h[i*size+bottom]-=v/8.0;
		h[left*size+top]-=v/8.0;
		h[left*size+bottom]-=v/8.0;
		h[right*size+top]-=v/8.0;
		h[right*size+bottom]-=v/8.0;
	
	for i in range(0,8):	
		Shallow_Wave(h,new_h);
		
	# 将h中的顶点位置重新储存回Mesh.
	for i in range(0,size):
		for j in range(0,size):
			X[i*size+j].y = h[i*size+j];
	var _mesh = ArrayMesh.new();
	_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES,mesh.surface_get_arrays(0));
	
	var mdt = MeshDataTool.new();
	mdt.create_from_surface(_mesh,0);
	for i in range(mdt.get_vertex_count()):
		mdt.set_vertex(i,X[i]);	
		
		# recalculate normal
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
			
	_mesh.surface_remove(0);
	mdt.commit_to_surface(_mesh);	
	self.mesh = _mesh;
