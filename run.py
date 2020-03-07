import pygame
from pygame.locals import *
import math
import random
import time
import sys
import copy
from queue import PriorityQueue

path_points = [(4,2),(4,18),(32,18),(32,4),(18,4),(18,32),(34,32)]
def regularize_map(mp):
	for i in range(37):
		for j in range(37):
			if i < 9 and j < 9 or i > 27 and j > 27:
				mp[i][j] = 0
			if (i,j) in path_points:
				mp[i][j] = 0

def dij(mp):
	regularize_map(mp)
	dist_list = []
	for i in range(len(path_points)-1):
		st = path_points[i]
		ed = path_points[i+1]
		dis = [[1e5 for _ in range(37)] for __ in range(37)]
		q = PriorityQueue()
		dis[st[0]][st[1]] = 0
		q.put((0,st))
		while not q.empty():
			_, p = q.get()
			dd = ((0,1),(0,-1),(1,0),(-1,0),(1,1),(-1,1),(-1,-1),(1,-1))
			for d in dd:
				nx = p[0] + d[0]
				ny = p[1] + d[1]
				if nx >= 0 and ny >= 0 and nx < 37 and ny < 37:
					if mp[nx][ny] == 0:
						if abs(d[0]) + abs(d[1]) == 1 or mp[nx][p[1]] == 0 or mp[p[0]][ny] == 0:
							if dis[nx][ny] >= 1e5:
								dis[nx][ny] = dis[p[0]][p[1]] + math.sqrt(d[0]*d[0]+d[1]*d[1])
								q.put((dis[nx][ny],(nx,ny)))
		dist_list.append(dis[ed[0]][ed[1]])
	return dist_list
def getline(lxd, xd, lyd, yd):
	lis = []
	dx = xd - lxd
	dy = yd - lyd
	print(dx, dy)
	if abs(dx) >= abs(dy):
		if xd < lxd:
			xd, lxd = lxd, xd
			yd, lyd = lyd, yd
		dx = xd - lxd
		dy = yd - lyd
		for x in range(lxd, xd+1):
			y = ((x - lxd) * (dy) + ((dx) // 2)) // max(dx, 1) + lyd
			lis.append((x,y))
	else:
		if yd < lyd:
			xd, lxd = lxd, xd
			yd, lyd = lyd, yd
		dx = xd - lxd
		dy = yd - lyd
		for y in range(lyd, yd+1):
			x = ((y - lyd) * (dx) + ((dy) // 2)) // max(dy, 1) + lxd
			lis.append((x,y))
	return lis
 
def main():
	pygame.init()
	# logo = pygame.image.load("xx.png")
	# pygame.display.set_icon(logo)
	pygame.display.set_caption("GemTD Map Editor (by curs0r)")
	screen_width = 675
	screen_height = 675
	screen = pygame.display.set_mode((screen_width, screen_height))

	# img = pygame.image.load("1.png");
	fnt = pygame.font.SysFont("", 30);

	pygame.event.set_allowed(pygame.KEYDOWN or pygame.MOUSEBUTTONUP or pygame.MOUSEBUTTONDOWN)
	running = True
	last_clock = time.clock()
	print(last_clock)
	fps_count = 0
	fps = 0

	stones = [[0 for _ in range(37)] for __ in range(37)]
	maze_length = 0
	dist_list = []
	

	cur_mouse = 0

	while running:
		fps_count += 1
		now_clock = time.clock()
		if now_clock - last_clock >= 1.0:
			last_clock = now_clock
			fps = fps_count
			fps_count = 0

		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				running = False
			elif event.type == pygame.KEYDOWN:
				print(event)
				if event.key == K_s:
					file = open("gem-sv1.txt", "w")
					for i in range(0,37):
						file.write(str(stones[i]) + "\n")
					print("save done!")
					file.close()
				elif event.key == K_o:
					try:
						file = open("gem-sv1.txt", "r")
						read_stones = []
						for i in range(0,37):
							line = eval(file.readline())
							ok = isinstance(line, list) and all(isinstance(a, int) for a in line) and len(line) == 37
							if not ok:
								print("data err", i, line)
								break
							read_stones.append(line)
						if len(read_stones) == 37:
							stones = read_stones
							print("load done!")
						file.close()
					except:
						print(sys.exc_info())
						pass
			elif event.type == pygame.MOUSEBUTTONDOWN:
				print(event)
				x = event.pos[0]
				y = event.pos[1]
				lxd = ((x - 4) // 18)
				lyd = ((y - 4) // 18)
				lmousebutton = 0
				if x >= 4 and y >= 4:
					if lxd >= 0 and lyd >= 0 and lxd < 39 and lyd < 39:
						lmousebutton = event.button
			elif event.type == pygame.MOUSEBUTTONUP:
				print(event)
				if lmousebutton == event.button:
					x = event.pos[0]
					y = event.pos[1]
					xd = ((x - 4) // 18)
					yd = ((y - 4) // 18)
					if x >= 4 and y >= 4:
						if xd >= 0 and yd >= 0 and xd < 37 and yd < 37:
							for p in getline(lxd, xd, lyd, yd):
								x, y = p[0], p[1]
								if event.button == 1:
									stones[x][y] = 1
								elif event.button == 3:
									stones[x][y] = 0
								else:
									stones[x][y] = 1 - stones[x][y]
		screen.fill((150,200,150))
		for i in range(0,37):
			for j in range(0, 37):
				color = [255,255,255]
				if stones[i][j] == 1:
					color = [0,0,0]
				if i < 9 and j < 9 or i > 27 and j > 27:
					color = [200,200,200]
				if (i,j) in path_points:
					color = [200,0,0]
				pygame.draw.rect(screen, color, pygame.Rect((5 + 18 * i, 5 + 18 * j),(16,16)))


		text = fnt.render("fps:" + str(fps), True, (255,255,255))
		screen.blit(text, (0, 0))

		dist_list = dij(stones)
		dist_list_rounded = [round(_,2) for _ in dist_list]
		text = fnt.render("part len:" + str(dist_list_rounded), True, (200,50,50))
		screen.blit(text, (0, 30))

		real_len = round(sum(dist_list), 2)
		text = fnt.render("sum len:" + str(real_len), True, (200,50,50))
		screen.blit(text, (0, 60))

		pygame.display.flip()

if __name__ == "__main__":
	main()