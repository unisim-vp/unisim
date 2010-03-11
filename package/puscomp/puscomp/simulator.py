# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
import sys
import os
import os.path
import shutil

class SimulatorClass:
	def __init__(self, filename, repository):
		self._status = True
		self._filename = filename
		self._unit_list = []
		xml = ET.parse(filename)
		root = xml.getroot()
		name = root.find("./name")
		self._name = name.text
		version = root.find("./version")
		self._version = version.text
		authors = root.findall("./authors/author")
		self._authors = []
		if authors:
			for author in authors:
				self._authors.append(author.text)
		files = root.findall("./files/file")
		self._files = []
		if files:
			for file in files:
				self._files.append(file.text)
		deps = root.findall("./depends/dep")
		self._units = dict()
		if deps:
			for dep in deps:
				# print("---> simulator")
				self._status = self._scanDependency(dep.text, repository)
				if not self._status:
					return
		self._cmake = None
		cmake = root.find("./cmake")
		if cmake is not None:
			self._cmake = cmake.text
			

	def getStatus(self):
		return self._status
		
	def _scanDependency(self, dependency, repository):
		if dependency in self._units.keys():
			return True
		unit = repository.getUnit(dependency)
		if unit is None:
			print('\nERROR: could not find unit "%s"' % (dependency))
			return False
		self._units[dependency] = unit
		for dep in unit.getDependencies():
			# print("---> %s" % (unit.getName()))
			if not self._scanDependency(dep, repository):
				return False
		return True

	def generate(self, config, output_path):
		# copy all the files
		self._generateCopyFiles(config, output_path)
		if not self._status:
			return
		# generate the cmake
		# self._generateCmake(config, output_path)
		self._generateCopyCMakes(config, output_path)
		if not self._status:
			return

	def _generateCopyFiles(self, config, output_path):
		for unit in self._units.values():
			rep_location = config.getRepositoryLocationFromName(unit.getRepositoryName())
			for file in unit.getFiles():
				abs_filename = os.path.join(rep_location, file)
				abs_out_filename = os.path.join(output_path, file)
				abs_out_path = os.path.dirname(abs_out_filename)
				if not os.path.exists(abs_out_path):
					os.makedirs(abs_out_path)
				if not os.path.exists(abs_filename):
					print("ERROR: '%s' does not exist" % (abs_filename))
					self._status = False
					return
				os.symlink(abs_filename, abs_out_filename)
		abs_path = os.path.dirname(self._filename)
		for file in self._files:
			abs_filename = os.path.join(abs_path, file)
			abs_out_filename = os.path.join(output_path, file)
			abs_out_path = os.path.dirname(abs_out_filename)
			if not os.path.exists(abs_out_path):
				os.makedirs(abs_out_path)
			if not os.path.exists(abs_filename):
				print("ERROR: '%s' does not exists" % (abs_filename))
				self._status = False
				return
			os.symlink(abs_filename, abs_out_filename)
		return
	
	def _generateCopyCMakes(self, config, output_path):
		for unit in self._units.values():
			print (" + %s" % (unit.getName()), end='')
			rep_location = config.getRepositoryLocationFromName(unit.getRepositoryName())
			rel_filename = unit.getCmake()
			if rel_filename is None:
				print (' (no cmake defined)')
			else:
				abs_filename = os.path.join(rep_location, rel_filename)
				if not os.path.exists(abs_filename):
					print (' (cmake file does not exists)')
				else:
					abs_out_filename = os.path.join(output_path, rel_filename)
					if not os.path.exists(os.path.dirname(abs_out_filename)):
						os.makedirs(os.path.dirname(abs_out_filename))
					os.symlink(abs_filename, abs_out_filename)
					print ('')
		print (" * simulator", end = '')
		if self._cmake is None:
			print (' (no cmake defined)')
		else:
			abs_filename = os.path.join(os.path.dirname(self._filename), self._cmake)
			if not os.path.exists(abs_filename):
				print (' (cmake file does not exists)')
			else:
				abs_out_filename = os.path.join(output_path, self._cmake)
				if not os.path.exists(os.path.dirname(abs_out_filename)):
					os.makedirs(os.path.dirname(abs_out_filename))
				os.symlink(abs_filename, abs_out_filename)
				print('')
				

	def _generateCmake(self, config, output_path):
		# copy cmake header
		cmake_path = os.path.abspath(os.path.dirname(sys.argv[0]))
		cmake_filename = os.path.join(cmake_path, "CMakeList.txt.header")
		out_cmake_filename = os.path.join(output_path, "CMakeLists.txt")
		shutil.copyfile(cmake_filename, out_cmake_filename)
		out_cmake_file = open(out_cmake_filename, 'a')
		sorted_units = self._sortUnits()
		for unit_name in sorted_units:
			self._generateCMakeFromUnit(self._units[unit_name], config, out_cmake_file)
		if self._cmake is not None:
			abs_path = os.path.dirname(self._filename)
			abs_filename = os.path.join(abs_path, self._cmake)
			inf = open(abs_filename, 'r')
			for line in inf.readlines():
				out_cmake_file.write(line)
			inf.close()
		out_cmake_file.close()
		cmake_filename = os.path.join(cmake_path, "CMakeList.txt.tail")
		self._appendFileToFile(out_cmake_filename, cmake_filename)
		return

	def _sortUnits(self):
		sorted_units = []
		for (name, unit) in self._units.items():
			sorted_units = self._sortUnitsRec(name, sorted_units)
		return sorted_units

	def _sortUnitsRec(self, name, sorted_units):
		if name in sorted_units:
			return sorted_units
		for dep in self._units[name].getDependencies():
			sorted_units = self._sortUnitsRec(dep, sorted_units)
		pos = 0
		for dep in self._units[name].getDependencies():
			possible_pos = sorted_units.index(dep)
			if possible_pos > pos:
				pos = possible_pos
		if pos is 0:
			sorted_units.insert(0, name)
		else:
			sorted_units.insert(pos + 1, name)
		return sorted_units
			
	def _generateCMakeFromUnit(self, unit, config, out_cmake_file):
		print ("+ %s" % (unit.getName()), end = '')
		base_path = config.getRepositoryLocationFromName(unit.getRepositoryName())
		rel_path = unit.getCmake()
		if rel_path is None:
			print (' -')
			return
		abs_path = os.path.join(base_path, rel_path)
		if not os.path.exists(abs_path):
			print (' *')
			return
		inf = open(abs_path, 'r')
		for line in inf.readlines():
			out_cmake_file.write(line)
		inf.close()
		print (' +')
		return 

	def _appendFileToFile(self, filename, in_filename):
		rfile = open(in_filename, 'r')
		file = open(filename, 'a')
		for line in rfile.readlines():
			file.write(line)
		rfile.close()
		file.close()

	def dump(self, pre):
		print('%sName: %s' % (pre, self._name))
		print('%sVersion: %s' % (pre, self._version))
		if len(self._authors) is not 0:
			if len(self._authors) is 1:
				print('%sAuthor:' % (pre), end = '')
			else:
				print('%sAuthors:' % (pre), end = '')
			for i in range(0, len(self._authors) - 1):
				print(' %s,' % (self._authors[i]), end = '')
			print(' %s' % (self._authors[len(self._authors) - 1]))
		print('%sFilename: %s' % (pre, self._filename))
		print('%sUnits:' % (pre))
		for unit in self._units.keys():
			print('%s  %s' % (pre, unit))
