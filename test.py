import atometa

# Predefined molecules
water = atometa.create_water()       # H2O
methane = atometa.create_methane()   # CH4
benzene = atometa.create_benzene()   # C6H6

# Periodic table
element = atometa.get_element("C")
print(element)
# {'symbol': 'C', 'name': 'Carbon', 'atomic_number': 6,
#  'mass': 12.011, 'radius': 0.77}