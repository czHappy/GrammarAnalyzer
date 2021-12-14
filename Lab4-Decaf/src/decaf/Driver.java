package decaf;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import decaf.tree.Tree;
import decaf.backend.Mips;
import decaf.dataflow.FlowGraph;
import decaf.error.DecafError;
import decaf.frontend.Lexer;
import decaf.frontend.Parser;
import decaf.machdesc.MachineDescription;
import decaf.scope.ScopeStack;
import decaf.tac.Functy;
import decaf.translate.Translater;
import decaf.typecheck.BuildSym;
import decaf.typecheck.TypeCheck;
import decaf.utils.IndentPrintWriter;

public final class Driver {

	private static Driver driver;

	private Option option;

	private List<DecafError> errors;

	private ScopeStack table;

	private Lexer lexer;

	private Parser parser;

	public ScopeStack getTable() {
		return table;
	}

	public static Driver getDriver() {
		return driver;
	}

	public Option getOption() {
		return option;
	}

	public void issueError(DecafError error) {
		errors.add(error);
	}

	// Only allow construction by Driver.main
	private Driver() {
	}

	/**
	 * 输出错误
	 */
	private void checkPoint() {
		if (errors.size() > 0) {
			Collections.sort(errors, new Comparator<DecafError>() {
				//按照坐标位置对错误进行排序
				@Override
				public int compare(DecafError o1, DecafError o2) {
					return o1.getLocation().compareTo(o2.getLocation());
				}

			});
			for (DecafError error : errors) {
				option.getErr().println(error);
			}
			System.exit(1);
		}
	}
	
	private void init() {
		lexer = new Lexer(option.getInput());
		parser = new Parser();
		lexer.setParser(parser);
		parser.setLexer(lexer);
		errors = new ArrayList<DecafError>();
		table = new ScopeStack();
	}

	private void compile() {
		//Tree.TopLevel顶层抽象语法树，类定义，全局
		Tree.TopLevel tree = parser.parseFile();
		checkPoint();
		//根据输入参数0 1 2 3 4决定编译到什么地方为止
		
		
		//阶段0 输出AST
		if (option.getLevel() == Option.Level.LEVEL0) {
			IndentPrintWriter pw = new IndentPrintWriter(option.getOutput(), 4);
			tree.printTo(pw);//自顶向下依次输出AST
			pw.close();
			return;
		}
		
		//阶段1 语义分析
		//Visitor模式建立符号表信息和静态语义检查
		BuildSym.buildSymbol(tree);
		checkPoint();
		
		TypeCheck.checkType(tree);
		checkPoint();
		//输出带标注的AST
		if (option.getLevel() == Option.Level.LEVEL1) {
			IndentPrintWriter pw = new IndentPrintWriter(option.getOutput(), 4);
			tree.globalScope.printTo(pw);
			pw.close();
			return;
		}
		
		//阶段2 中间代码生成  生成TAC
		PrintWriter pw = new PrintWriter(option.getOutput());
		
		//翻译
		Translater tr = Translater.translate(tree);
		checkPoint();
		
		
		if (option.getLevel() == Option.Level.LEVEL2) {
			tr.printTo(pw);
			pw.close();
			return;
		}
		
		//阶段3 数据流图建立
		List<FlowGraph> graphs = new ArrayList<FlowGraph>();
		for (Functy func : tr.getFuncs()) {
			graphs.add(new FlowGraph(func));
		}

		if (option.getLevel() == Option.Level.LEVEL3) {
			for (FlowGraph g : graphs) {
				g.printLivenessTo(pw);
				pw.println();
			}
			pw.close();
			return;
		}

		//阶段4 汇编代码生成
		MachineDescription md = new Mips();
		md.setOutputStream(pw);
		md.emitVTable(tr.getVtables());
		for (int i = 0; i < 3; i++) {
			pw.println();
		}
		md.emitAsm(graphs);
		pw.close();
	}

	public static void main(String[] args) throws IOException {
		args = new String[] {
				 "E:\\test.decaf",
				 "-l", "2" };
		driver = new Driver();
		driver.option = new Option(args);
		driver.init();
		driver.compile();
	}
}
