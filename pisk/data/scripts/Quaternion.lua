
local Quaternion = {}

function Quaternion:new(x, y, z, w)
	local obj = { x = x, y = y, z = z, w = w }

	function obj:length()
		return (self.x^2 + self.y^2 + self.z^2 + self.w^2)^0.5
	end

	function obj:normalize()
		local l = self:length()
		if l == 0 then error("normalize: Quaternion has zero length") end
		return Quaternion:new(
			self.x / l,
			self.y / l,
			self.z / l,
			self.w / l
		)
	end

	function obj:mul(q)
		return Quaternion:new(
			self.w * q.x + self.x * q.w + self.y * q.z - self.z * q.y,
			self.w * q.y - self.x * q.z + self.y * q.w + self.z * q.x,
			self.w * q.z + self.x * q.y - self.y * q.x + self.z * q.w,
			self.w * q.w - self.x * q.x - self.y * q.y - self.z * q.z
		)
	end

	function obj:invert()
		return Quaternion:new(
			-self.x,
			-self.y,
			-self.z,
			self.w
		)
	end

	function obj:rotate_vector(v)
		local qv = Quaternion:new(v.x, v.y, v.z, 0)
		local r = self:mul(qv):mul(qn:invert())
		r.w = nil
		return r
	end

	setmetatable(obj, self)
	self.__index = self
	return obj
end

function Quaternion.length(q)
	return (q.x^2 + q.y^2 + q.z^2 + q.w^2) ^ 0.5
end

function Quaternion.normalize(q)
	local l = Quaternion.length(q)
	if l == 0 then error("normalize: quaternion has zero length") end
	
	return {
		x = q.x / l,
		y = q.y / l,
		z = q.z / l,
		w = q.w / l,
	}
end

function Quaternion.mul(q1, q2)
	return {
		x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
		w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
	}
end

function Quaternion.invert(q)
	return {
		x = -q.x,
		y = -q.y,
		z = -q.z,
		w =  q.w,
	}
end
function Quaternion.rotate_vector(q, v, invert)
	local qv = {
		x = v.x,
		y = v.y,
		z = v.z,
		w = 0,
	}
	local nq = Quaternion.normalize(q)
	local iq = Quaternion.invert(nq)
	if invert then
		local r = Quaternion.mul(Quaternion.mul(iq, qv), nq)
		r.w = nil
		return r
	else
		local r = Quaternion.mul(Quaternion.mul(nq, qv), iq)
		r.w = nil
		return r
	end
end

return Quaternion

