import { useNavigate } from "react-router-dom";

export default function Landing() {
  const navigator = useNavigate();

  return (
    <div className="min-h-screen w-full flex justify-center items-center gap-3">
      <div className="flex text-6xl font-medium tracking-tighter gap-0.5">
        <h1>Air</h1>
        <h1 className="text-orange-500">Q</h1>
      </div>

      <button
        onClick={() => navigator("/dashboard")}
        className="mt-2 bg-orange-500 text-white px-5 py-1.5 rounded-xl font-medium cursor-pointer"
      >
        Get Started &gt;
      </button>
    </div>
  );
}
