# OpenGL

invalid render buffer,排查后发现是忘记generate renderbuffer id就bind。
invalid enum ,错误原因，过滤设置错误。
第三人称视角。camera的lookat矩阵改为从character的Front指定的后方看向character，相应的csm也要将分割视锥体的中心从camera的position改为前面的位置，方向还是Front。
shader初始化必须在glwindows之后。
