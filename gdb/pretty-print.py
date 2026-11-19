import gdb
import gdb.printing
import gdb.types

pp = gdb.printing.RegexpCollectionPrettyPrinter('loki')

class sv(gdb.ValuePrinter):
    def __init__(self, val):
        self.__val = val

    def display_hint(self):
        return 'string'

    def to_string(self):
        len = int(self.__val['length'])
        return self.__val['s'].string(length=len)

class src_loc(gdb.ValuePrinter):
    def __init__(self, val):
        self.__val = val

    def display_hint(self):
        filename = str(self.__val['filename'])[1:-1]
        if 0 == len(filename):
            return None
        return 'string'

    def to_string(self):
        filename = str(self.__val['filename'])[1:-1]
        if 0 == len(filename):
            return '{}'
        return f'{filename}:{self.__val['line']}:{self.__val['column']}'

class token(gdb.ValuePrinter):
    def __init__(self, val):
        self.__val = val

    def children(self):
        kind = str(self.__val['type'])
        match kind:
            case 'TT_NONE':
                return iter([])
            case _:
                return [(field.name, self.__val[field.name]) for field in self.__val.type.fields()]


pp.add_printer('sv',     '^string$', sv)
pp.add_printer('qbe_sv', '^qbe_sv$', sv)
pp.add_printer('lex_sv', '^lex_sv$', sv)

pp.add_printer('src_loc', '^src_loc$', src_loc)
pp.add_printer('token',   '^token$',   token)

gdb.printing.register_pretty_printer(gdb.current_objfile(), pp, replace=True)
