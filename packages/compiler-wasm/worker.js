import { createCompiler } from "./index.js";

let compiler;
const sessions = new Map();

function session(id) {
  const value = sessions.get(id);
  if (!value) throw new Error(`unknown compiler session ${id}`);
  return value;
}

self.onmessage = async event => {
  const { id, method, args = [] } = event.data;
  try {
    if (!compiler) compiler = await createCompiler();
    let value;
    switch (method) {
      case "createSession":
        value = crypto.randomUUID();
        sessions.set(value, compiler.createSession());
        break;
      case "disposeSession":
        session(args[0]).dispose();
        sessions.delete(args[0]);
        value = true;
        break;
      case "putSource": value = session(args[0]).putSource(...args.slice(1)); break;
      case "removeSource": value = session(args[0]).removeSource(...args.slice(1)); break;
      case "compile": value = session(args[0]).compile(...args.slice(1)); break;
      case "parse": value = session(args[0]).parse(...args.slice(1)); break;
      case "analyze": value = session(args[0]).analyze(...args.slice(1)); break;
      case "format": value = session(args[0]).format(...args.slice(1)); break;
      default: throw new Error(`unknown compiler method ${method}`);
    }
    self.postMessage({ id, value });
  } catch (error) {
    self.postMessage({ id, error: error instanceof Error ? error.message : String(error) });
  }
};
